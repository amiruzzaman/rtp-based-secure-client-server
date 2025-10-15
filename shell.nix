{ pkgs ? import <nixpkgs> {} }:
    with pkgs; pkgs.mkShell {
        name = "soydev-tools-0.0.1";
        nativeBuildInputs = [
            # pkgconf 
        ];
        buildInputs = with pkgs; [
            libevent
        ];
        packages = with pkgs; [
            cmake
            ninja
        ];
    }
