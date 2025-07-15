{
  description = "Automated build flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { self, nixpkgs, }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    kernel = pkgs.linuxPackages.kernel;
    lib = pkgs.lib;

    sharedNativeBuildInputs = [
      pkgs.cmake
      pkgs.python3
      pkgs.gcc
      pkgs.gnumake
      pkgs.binutils
    ];

    pico-sdk = pkgs.fetchgit {
      url = "https://github.com/raspberrypi/pico-sdk.git";
      rev = "2.1.1";
      sha256 = "8ru1uGjs11S2yQ+aRAvzU53K8mreZ+CC3H+ijfctuqg=";
      fetchSubmodules = true;
    };

    libevdi = pkgs.stdenv.mkDerivation rec {
      pname = "libevdi";
      version = "1.14.10";

      src = pkgs.fetchFromGitHub {
        owner = "DisplayLink";
        repo = "evdi";
        rev = "v1.14.10";
        sha256 = "sha256-xB3AHg9t/X8vw5p7ohFQ+WuMjb1P8DAP3pROiwWkVPs=";
      };

      nativeBuildInputs = [ pkgs.cmake pkgs.pkg-config pkgs.gcc pkgs.libdrm ];

      dontConfigure = true;

      cmakeFlags = [
        "-DEVKMD_DISABLE_TESTS=ON"
        "-DEVKMD_BUILD_MODULE=OFF"
      ];

      # We only want libevdi, not kernel module
      sourceRoot = "source/library";

      installPhase = ''
        mkdir -p $out/include/libevdi
        mkdir -p $out/lib
        cp evdi_lib.h $out/include/libevdi/
        cp libevdi.so* $out/lib/
      '';

      meta = with pkgs.lib; {
        description = "Userspace library for evdi (DisplayLink virtual display)";
        license = licenses.gpl2Plus;
        maintainers = with maintainers; [ ];
      };
    };
  in {
    packages.x86_64-linux = {
      display = pkgs.stdenv.mkDerivation {
        name = "display";
        src = ./src/display;
        nativeBuildInputs = sharedNativeBuildInputs ++ [
          pkgs.gcc-arm-embedded
          pkgs.picotool
        ];

        configurePhase = ''
          echo "Configuration..."
          mkdir build
          cd build
          cmake .. -DPICO_SDK_PATH=${pico-sdk}
        '';

        buildPhase = ''
          echo "Building..."
          make -j$(nproc)
          cd ..
        '';

        installPhase = ''
          echo "Installation..."
          mkdir -p $out
          cp build/display.uf2 $out/
          echo "Firmware copied to $out/display.uf2"
        '';
      };

      usb_echo = pkgs.stdenv.mkDerivation {
        name = "usb_echo";
        src = ./src/usb_echo;
        nativeBuildInputs = sharedNativeBuildInputs ++ [
          pkgs.gcc-arm-embedded
          pkgs.picotool
        ];

        configurePhase = ''
          echo "Configuration..."
          mkdir build
          cd build
          cmake .. -DPICO_SDK_PATH=${pico-sdk}
        '';

        buildPhase = ''
          echo "Building..."
          make -j$(nproc)
          cd ..
        '';

        installPhase = ''
          echo "Installation..."
          mkdir -p $out
          cp build/usb_echo.uf2 $out/
          echo "Firmware copied to $out/usb_echo.uf2"
        '';
      };

      usb_display = pkgs.stdenv.mkDerivation {
        name = "usb_display";
        src = ./src/usb_display;
        nativeBuildInputs = sharedNativeBuildInputs ++ [
          pkgs.gcc-arm-embedded
          pkgs.picotool
        ];

        configurePhase = ''
          echo "Configuration..."
          mkdir build
          cd build
          cmake .. -DPICO_SDK_PATH=${pico-sdk}
        '';

        buildPhase = ''
          echo "Building..."
          make -j$(nproc)
          cd ..
        '';

        installPhase = ''
          echo "Installation..."
          mkdir -p $out
          cp build/usb_display.uf2 $out/
          echo "Firmware copied to $out/usb_display.uf2"
        '';
      };

      usb_echo_driver = pkgs.stdenv.mkDerivation {
        name = "usb_echo_driver";
        src = ./src/usb_echo_driver;
        nativeBuildInputs = sharedNativeBuildInputs;
        dontConfigure = true;

        buildPhase = ''
          echo "Building..."
          make -C ${kernel.dev}/lib/modules/${kernel.modDirVersion}/build M=$(pwd) modules
        '';

        installPhase = ''
          echo "Installation..."
          mkdir -p $out
          cp usb_echo_driver.ko $out/
          echo "Loadable kernel module copied to $out/usb_echo_driver.ko"
        '';
      };

      usb_display_driver = pkgs.stdenv.mkDerivation {
        name = "usb_display_driver";
        src = ./src/usb_display_driver;
        nativeBuildInputs = sharedNativeBuildInputs ++ [
          pkgs.libusb1
          libevdi
        ];
        dontConfigure = true;

        buildPhase = ''
          gcc -o usb_display_driver usb_display_driver.c \
          -I${libevdi}/include \
          -I${libevdi}/lib -levdi \
          -lusb-1.0
        '';

        installPhase = ''
          echo "Installation..."
          mkdir -p $out
          cp usb_display_driver $out/
          echo "Loadable kernel module copied to $out/usb_display_driver"
        '';
      };

      defaultPackage.x86_64-linux = self.packages.x86_64-linux.display;
    };
  };
}

