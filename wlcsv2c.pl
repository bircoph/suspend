#!/usr/bin/perl
# Rodolfo Garcia kix_at_kix.es
# suspend database management v1.0

use Switch;

switch ($#ARGV) {
  case 0 {
    my $fin = $ARGV[0];
    open(INF, "<$fin") or die "Could not open input file\n";
    open(OUF, ">&STDOUT") or die "Could not open output file\n";
  }
  case 1 {
    my $fin = $ARGV[0];
    my $fout = $ARGV[1];
    open(INF, "<$fin") or die "Could not open input file\n";
    open(OUF, ">$fout") or die "Could not open output file\n";
  }
  else {
    my $msg = $0 . " <input-file> [output-file]";
    die $msg;
  }
}

&print_c_header;

my $sys_vendor = "";
my $sys_product = "";
my $sys_version = "";
my $bios_version = "";
my $flags = "";
my $comments = "";

while(<INF>) {
  if (/"([^"]*)",\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)",\s*([^,]*),\s*(.*)/) {
    $sys_vendor = $1;
    $sys_product = $2;
    $sys_version = $3;
    $bios_version = $4;
    $flags = $5;
    $comments = $6;

    printf OUF "\t$comments\n";
    printf OUF "\t{ \"" . $sys_vendor . "\",\t\"" . $sys_product . "\",\t\"" . $sys_version . "\",\t\"" . $bios_version . "\",\t" . $flags . " },\n";
  }
}

&print_c_footer;

close(INF);
close(OUF);

sub print_c_header
{
  ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);

  printf OUF "/* whitelist.c\n";
  printf OUF " * whitelist of machines that are known to work somehow\n";
  printf OUF " * and all the workarounds\n";
  printf OUF " * Script generated file, please do not edit by hand\n";
  printf OUF " */\n";
  printf OUF "\n";
  printf OUF "\#include \"config.h\"\n";
  printf OUF "\#include \<stdlib.h\>\n";
  printf OUF "\#include \"whitelist.h\"\n";
  printf OUF "\n";

  printf OUF "char *whitelist_version = \"\$Id: whitelist.c ver. ";
  printf OUF "%4d%02d%02d %02d:%02d:%02d",$year+1900,$mon+1,$mday,$hour,$min,$sec;
  printf OUF " automatic generation - kix - Exp \$\";\n";
  printf OUF "\n";
  printf OUF "struct machine_entry whitelist[] = {\n";
}

sub print_c_footer
{
  print OUF "\t{ NULL, NULL, NULL, NULL, 0 }\n";
  print OUF "};\n";
}
