#!/bin/sh

echo "Updating project with the lastest patches."
echo -n "Determining project version ... "
eval `cat ProjectVersion`

if test -z "$MAJORVERSION" -o -z "$MINORVERSION" -o -z "$BUILDVERSION" -o -z "$PATCHLEVEL"; then
   echo "error"
   echo "Cannot determine project version."
   exit 1
fi

echo -n "$MAJORVERSION.$MINORVERSION.$BUILDVERSION"
if test $PATCHLEVEL -ne 0; then
   echo -n ".$PATCHLEVEL"
fi
echo

PATCHLEVEL=$(($PATCHLEVEL + 1));
while wget -q $PATCH_DIR/fe4-$MAJORVERSION.$MINORVERSION.$BUILDVERSION.$PATCHLEVEL-jay.patch; do
   echo -n "Applying patch fe4-$MAJORVERSION.$MINORVERSION.$BUILDVERSION.$PATCHLEVEL-jay.patch ... "
   if cat fe4-$MAJORVERSION.$MINORVERSION.$BUILDVERSION.$PATCHLEVEL-jay.patch | patch -p0 -E -s; then
      echo "ok"
   else
      echo "error"
      echo "Could not apply patch. Please consult Jay about this."
      exit 1
   fi
   PATCHLEVEL=$(($PATCHLEVEL + 1));
done
eval `cat ProjectVersion`
echo -n "Our new project version is ... "
echo -n "$MAJORVERSION.$MINORVERSION.$BUILDVERSION"
if test $PATCHLEVEL -ne 0; then
   echo -n ".$PATCHLEVEL"
fi
echo
echo "Update is complete."

exit 0
