{
  description = "The Balsa Asynchronous Framework";

  outputs = { self, nixpkgs }: rec {
    packages.x86_64-linux.balsa =
      with import nixpkgs { system = "x86_64-linux"; };

      stdenv.mkDerivation {
        pname   = "balsa";
        version = "4.0";
        src     = self;

        dontDisableStatic = true;

        buildInputs = [
          gnutar gzip gnumake gcc coreutils gawk
          gnused gnugrep glib gtk2 gmp4 guile pkg-config
        ];
      };

    defaultPackage.x86_64-linux =
      packages.x86_64-linux.balsa;

    checks.build = packages.x86_64-linux.balsa;
  };
}
