#!/bin/sh
set -e

spec=`ls tools/tizen/*.spec`
version=`rpm --query --queryformat '%{version}\n' --specfile $spec`

name=`echo $name|cut -d" " -f 1`
version=`echo $version|cut -d" " -f 1`

name=iotivity
tinycbor_url='https://github.com/01org/tinycbor.git'

which git
which rpm
which gbs

rm -rf $name-$version

builddir=`pwd`
sourcedir=`pwd`

echo `pwd`

grep 'email *= *' ~/.gitconfig \
    || git config --global user.email "you@example.com"

grep 'name *= *' ~/.gitconfig \
    || git config --global user.name "Your Name"

# Clean tmp directory.
rm -rf ./tmp

# Create directory structure for GBS Build
mkdir ./tmp
mkdir ./tmp/extlibs/
mkdir ./tmp/packaging
cp -R ./build_common $sourcedir/tmp
cp -R ./examples $sourcedir/tmp

# Fetch tinycbor sources if not available locally
if [ ! -e 'extlibs/tinycbor/tinycbor' ] ; then
    echo "warning: fetching online sources may not be reproductible"
    git clone $tinycbor_url extlibs/tinycbor/tinycbor
fi

# tinycbor is available as soft-link, so copying with 'dereference' option.
cp -LR ./extlibs/tinycbor $sourcedir/tmp/extlibs
rm -rf $sourcedir/tmp/extlibs/tinycbor/tinycbor/.git

cp -R ./extlibs/cjson $sourcedir/tmp/extlibs
cp -R ./extlibs/tinydtls $sourcedir/tmp/extlibs
cp -R ./extlibs/sqlite3 $sourcedir/tmp/extlibs
cp -R ./extlibs/timer $sourcedir/tmp/extlibs
cp -R ./extlibs/rapidxml $sourcedir/tmp/extlibs
cp -R ./resource $sourcedir/tmp
cp -R ./service $sourcedir/tmp
cp ./extra_options.scons $sourcedir/tmp
cp ./tools/tizen/*.spec ./tmp/packaging
cp ./tools/tizen/*.manifest ./tmp/packaging
cp ./SConstruct ./tmp
cp ./LICENSE.md ./tmp

# copy dependency RPMs and conf files for tizen build
cp ./tools/tizen/*.rpm ./tmp
cp ./tools/tizen/.gbs.conf ./tmp

cp -R $sourcedir/iotivity.pc.in $sourcedir/tmp

cd $sourcedir/tmp

echo `pwd`
rm -rf ./extlibs/tinycbor/tinycbor/.git*

# Initialize Git repository
if [ ! -d .git ]; then
   git init ./
   git add ./
   git commit -m "Initial commit"
fi

echo "Calling core gbs build command"
gbscommand="gbs build -A armv7l -B ~/GBS-ROOT-OIC --include-all --repository ./"
echo $gbscommand
if eval $gbscommand; then
   echo "Build is successful"
else
   echo "Build failed!"
   exit 1
fi

cd $sourcedir
rm -rf $sourcedir/tmp

exit 0
