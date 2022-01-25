{ pkgs ? import <nixpkgs> { } }: with pkgs;
stdenv.mkDerivation {
  name = "balsa-shell";
  buildInputs = [ (callPackage ./default.nix { }) ];
}
