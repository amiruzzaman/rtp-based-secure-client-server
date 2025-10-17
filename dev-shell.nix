{ pkgs ? import <nixpkgs> {} }:
    with pkgs; stdenv.mkDerivation {
        name = "soydev-tools-0.0.1";
        system = builtins.currentSystem;
        nativeBuildInputs = [
            clang-tools
            cmake
            ninja
            pkgconf 
        ];
        buildInputs = [
            libevent
        ];
    }
