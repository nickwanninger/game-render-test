{
  description = "Game Render Test";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};

          runInputs = with pkgs; [
            SDL2
            lua
          ];

          buildInputs = with pkgs; runInputs ++ [
            makeWrapper

            bashInteractive
            darwin.apple_sdk.frameworks.OpenGL
          ];

        in
        with pkgs; {
          # devShell = mkShell {
          #   inherit buildInputs;


          #   LOCALE_ARCHIVE = "${glibcLocales}/lib/locale/locale-archive";
          #   hardeningDisable = ["all"];

          #   shellHook = ''
          #     source $PWD/enable
          #   '';
          # };

          packages.default = pkgs.stdenv.mkDerivation {
            name = "game-render-test";
            inherit buildInputs;

            src = pkgs.nix-gitignore.gitignoreSource [] ./.;
            installPhase = ''
              set -x

              pwd
              ls -la .
              cp -r ./ $out/

              set +x
            '';
          };
        }
      );
}
