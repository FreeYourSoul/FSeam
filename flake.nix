{
  description = "Chaiscript library.";

  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in rec {

        defaultPackage = pkgs.stdenv.mkDerivation {
          pname = "fseam";
          version = "1.0.0";
          src = pkgs.fetchgit {
            url = "https://github.com/FreeYourSoul/FSeam.git";
            rev = "577de1da89bf553a804fe8ed47fcc6fa1b46e1b5";
            sha256 = "sha256-X7fXxAh6x631O0IOEBIflSFZM3jBuxC6gDrdqMnXqiA=";
          };

          cmakeFlags = [ "-DFSEAM_BUILD_TESTS=OFF" ];

          doCheck = false;

          enableParallelBuilding = false;

          buildInputs = with pkgs; [ cmake catch2 python3 python3Packages.ply ];
        };

        chaiscript = defaultPackage;

      });
}
