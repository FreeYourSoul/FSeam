{ rev ? "577de1da89bf553a804fe8ed47fcc6fa1b46e1b5", stdenv, cmake, catch2
, python3, python37Packages }:

stdenv.mkDerivation rec {

  version = "1.0.0";
  pname = "fseam";

  src = builtins.fetchGit {
    url = "https://github.com/FreeYourSoul/FSeam.git";
    rev = "${rev}";
  };

  cmakeFlags = [ "-DFSEAM_BUILD_TESTS=OFF" ];

  doCheck = false;

  enableParallelBuilding = false;

  buildInputs = [ cmake catch2 python3 python37Packages.ply ];
}
