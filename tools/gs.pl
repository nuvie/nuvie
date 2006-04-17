#!/usr/bin/perl

# Generate spell stubs from basic spell info.

$count=0;
while (<>) {
($definitions[$count],$definitions[$count+5])=unpack("A24 x A24",$_);
$count++; 
if ( (not ($count%5)) and ($count%10))
{
$count+=5;
}
}

$count=0;
$count2=0;
foreach $definition (@definitions) {
  ($name,$invo,$reag)=unpack("xx A12 x A4 x A4",$definition);

  print "# $count \nname: $name\ninvocation: $invo\nreagents: $reag\nscript:\\\n";
  print "++ This is the $name spell stub ++ _display _end\n~\n";

  $count++;
  $count2++; if (not $count2%10) { $count+=6; print "# empty slots\n~\n~\n~\n~\n~\n~\n"}
}
