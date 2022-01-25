{ stdenv, gnutar, gzip, gnumake, gcc, coreutils, gawk
, gnused, gnugrep, glib, gtk2, gmp4, guile, pkg-config
}: stdenv.mkDerivation {
  pname   = "balsa";
  version = "4.0";
  src     = ./.;

  dontDisableStatic = true;

  buildInputs = [
    gnutar gzip gnumake gcc coreutils gawk
    gnused gnugrep glib gtk2 gmp4 guile pkg-config
  ];
}
