#!/usr/bin/perl -w
# $Id: qparse.pl,v 1.1 2002/03/23 05:01:11 j10 Exp $
# Quick Parser for FE4

print "FE4 Quick Parser (c)2002 Jay\nCtrl+D to quit.\n\n";
# Read each line and convert
while(<>) {
   chomp($_); # chomp nl
   for(my $i = 0; $i < length($_); $i++) {
      my $c = ord(substr($_,$i,1));

      if(&isupper($c)) { $c -= ord('A') - 0x10; }
      elsif(&islower($c)) { $c -= ord('a') - 0x2A; }
      elsif(&isdigit($c) && $c != ord('0')) { $c -= ord('0') - 0xB1; }
      elsif($c == ord('0')) { $c = 0xB1; }
      elsif($c == ord('.')) { $c = 0xBC; }
      elsif($c == ord(',')) { $c = 0xBB; }
      elsif($c == ord('?')) { $c = 0xBE; }
      elsif($c == ord('!')) { $c = 0xBD; }
      elsif($c == ord(' ')) { $c = 0xC5; }
      elsif($c == ord('-')) { $c = 0xB0; }
      elsif($c == ord('{')) { $c = 0xC0; }
      elsif($c == ord('}')) { $c = 0xC1; }
      elsif($c == ord('(')) { $c = 0xC2; }
      elsif($c == ord(')')) { $c = 0xC3; }
      elsif($c == ord('<')) { $c = 0xC4; }
      else { print "Can't convert characters.\n"; }

      printf("%.2X",$c);
   }
   print "\n";
}

sub isupper
{
   my $c = shift;
   return ($c >= ord('A') && $c <= ord('Z'));
}

sub islower
{
   my $c = shift;
   return ($c >= ord('a') && $c <= ord('z'));
}

sub isdigit
{
   my $c = shift;
   return ($c >= ord('0') && $c <= ord('9'));
}

0;
