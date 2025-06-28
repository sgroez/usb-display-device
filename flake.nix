{
  description = "Development shell and automated build flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
  };

  outputs = { self, nixpkgs }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;

    pico-sdk = pkgs.fetchgit {
      url = "https://github.com/raspberrypi/pico-sdk.git";
      rev = "2.1.1";
      sha256 = "0000000000000000000000000000000000000000000000000000";
      fetchSubmodules = true;
    };
  in {
    devShells.default = pkgs.mkShell {
      nativeBuildInputs = [
        pkgs.cmake
        pkgs.python3
        pkgs.gcc
        pkgs.pkgs.gnumake
        pkgs.binutils
        pkgs.gcc-arm-embedded
      ];
    };

    packages.x86_64-linux.default = pkgs.stdenv.mkDerivation {
      name = "usb-display-device-firmware";
      src = ./src;
      nativeBuildInputs = [
        pkgs.cmake
        pkgs.python3
        pkgs.gcc
        pkgs.pkgs.gnumake
        pkgs.binutils
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
        cd build
        make -j$(nproc)
      '';

      installPhase = ''
        echo "Installation..."
        mkdir -p $out
        cp build/blink.uf2 $out/
        echo "Firmware copied to $out/blink.uf2"
      '';
    };
  };
}

