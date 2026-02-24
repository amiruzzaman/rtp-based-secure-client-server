{
  description = "Build and dev env";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    systems = [
      "x86_64-linux"
      "aarch64-linux"
    ];
  in
  {
    devShells = (builtins.listToAttrs (builtins.map (system:
      let
        pkgs = nixpkgs.legacyPackages."${system}";
        buildInputs = with pkgs; [
          libevent
        ];
        nativeBuildInputs = with pkgs; [
          cmake
          ninja
          pkgconf
        ];
      in
      {
        name = system;
        value = {
          default = pkgs.mkShell {
            inherit buildInputs;
            inherit nativeBuildInputs;
          };
          devenv = pkgs.mkShell {
            inherit buildInputs;
            nativeBuildInputs = nativeBuildInputs ++ [
              pkgs.clang-tools
            ];
          };
        };
      }
    ) systems));
  };
}
