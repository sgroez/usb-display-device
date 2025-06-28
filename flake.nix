{
  description = "Development shell and automated build flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";

  outputs = { self, nixpkgs }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
    pico_sdk_path = ./pico-sdk;
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
      ];

      configurePhase = ''
        echo "Configuration..."
        mkdir build
        cd build
        cmake ..
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

