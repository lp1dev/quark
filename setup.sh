#!/usr/bin/env bash

# Locally installing lexbor sources to build for the vita
curl -L https://github.com/lexbor/lexbor/archive/refs/tags/v2.3.0.zip -o lexbor.zip
unzip lexbor.zip
mv lexbor-2.3.0/source/lexbor .
rm -frv lexbor.zip lexbor-2.3.0
rm -fr lexbor/ports/windows_nt # We are removing windows-related sources

# Locally installing duktape sources to build for the vita
curl -L https://duktape.org/duktape-2.7.0.tar.xz -o duktape.tar.xz
tar xvf duktape.tar.xz
mv duktape-2.7.0 duktape
rm -frv duktape.tar.xz
