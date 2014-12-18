#!/bin/bash

PYDIR=python_osx
OUTDIR=../distrib/python_osx

rm -rf $OUTDIR
mkdir $OUTDIR

cp $PYDIR/_mmapi.so $OUTDIR
cp $PYDIR/setup.py $OUTDIR
cp $PYDIR/mmapi.py $OUTDIR
cp $PYDIR/mmRemote.py $OUTDIR
cp $PYDIR/test.py $OUTDIR

cp StoredCommands.h $OUTDIR


