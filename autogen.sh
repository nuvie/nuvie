#! /bin/sh

DIE=0

# Check for availability
(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo "**Error**: You must have 'autoconf' installed to compile Underworld Adventures."
  DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo "**Error**: You must have 'automake' installed to compile Underworld Adventures."
  DIE=1
  NO_AUTOMAKE=yes
}
# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo "**Error**: Missing 'aclocal'.  The version of 'automake'"
  echo "installed doesn't appear recent enough."
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

aclocalincludes=""
if test -d "/usr/local/share/aclocal"; then
  if test "/usr/local/share/aclocal" != `aclocal --print-ac-dir`; then
    aclocalincludes="$aclocalincludes -I /usr/local/share/aclocal"
  fi
fi

#if test -d "$HOME/share/aclocal"; then
#  if test "$HOME/share/aclocal" != `aclocal --print-ac-dir`; then
#    aclocalincludes="$aclocalincludes -I $HOME/share/aclocal"
#  fi
#fi

echo "Cleaning up..."

# clean up generated files
rm -f configure config.log config.guess config.sub config.status
rm -f depcomp compile
rm -f aclocal.m4
rm -f install-sh ltmain.sh missing mkinstalldirs
rm -f Makefile.in Makefile
rm -f tools/Makefile.in tools/Makefile

echo "Generating files..."

aclocal $aclocalincludes
autoheader
automake --copy --force --add-missing --foreign
autoconf

echo "You are now ready to run ./configure"
