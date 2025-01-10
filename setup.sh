#!/usr/bin/env bash

# Locally installing lexbor sources to build for the vita (HTML/CSS Support)
curl -L https://github.com/lexbor/lexbor/archive/refs/tags/v2.3.0.zip -o lexbor.zip
unzip lexbor.zip
mv lexbor-2.3.0/source/lexbor .
rm -frv lexbor.zip lexbor-2.3.0
rm -fr lexbor/ports/windows_nt # We are removing windows-related sources

# Locally installing duktape sources to build for the vita (JS Support)
curl -L https://duktape.org/duktape-2.7.0.tar.xz -o duktape.tar.xz
tar xvf duktape.tar.xz
mv duktape-2.7.0 duktape
rm -frv duktape.tar.xz

# Locally installing tinypy sources to build for the vita (Python Support)
# Temporarily disabled
#curl -L https://github.com/philhassey/tinypy/archive/refs/tags/1.1.zip -o tinypy.zip
#unzip tinypy.zip -d tinypy_tmp
#mv tinypy_tmp/tinypy-1.1/tinypy/ .
#rm -frv tinypy_tmp tinypy.zip

