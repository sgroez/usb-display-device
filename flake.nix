{
  description = "Automated build flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { self, nixpkgs, }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;

    sharedNativeBuildInputs = [
      pkgs.cmake
      pkgs.python3
      pkgs.gcc
      pkgs.gnumake
      pkgs.binutils
      pkgs.gcc-arm-embedded
      pkgs.picotool
    ];

    pico-sdk = pkgs.fetchgit {
      url = "https://github.com/raspberrypi/pico-sdk.git";
      rev = "2.1.1";
      sha256 = "8ru1uGjs11S2yQ+aRAvzU53K8mreZ+CC3H+ijfctuqg=";
      fetchSubmodules = true;
    };

    sharp-display-patched = pkgs.stdenv.mkDerivation {
      pname = "sharp-memory-display-driver";
      version = "a29788c";

      src = pkgs.fetchgit {
        url = "https://github.com/Piorkos/sharp-memory-display-driver.git";
        rev = "118c50a6c648c41989b22b29922b3bf8ba29788c";
        sha256 = "sha256-j0JTY+fDRmvZ/m7Lc4Sw3XgDiveaqjE/BuSOSuB2EGc=";
      };

      patches = [ 
        ./patches/remove_example_auto_set_url.patch
      ];

      dontBuild = true;
      dontConfigure = true;

      installPhase = ''
        mkdir -p $out
        cp -r . $out/
      '';
    };
  in {
    packages.x86_64-linux = {
      display = pkgs.stdenv.mkDerivation {
        name = "display";
        src = ./src/display;
        nativeBuildInputs = sharedNativeBuildInputs;

        configurePhase = ''
          echo "Configuration..."
          mkdir build
          cd build
          cmake .. -DPICO_SDK_PATH=${pico-sdk} -DDISPLAY_DRIVER_PATH=${sharp-display-patched}
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

      usb = pkgs.stdenv.mkDerivation {
        name = "usb";
        src = ./src/usb;
        nativeBuildInputs = sharedNativeBuildInputs;

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
          cp build/usb.uf2 $out/
          echo "Firmware copied to $out/usb.uf2"
        '';
      };

      defaultPackage.x86_64-linux = self.packages.x86_64-linux.display;
    };
  };
}

