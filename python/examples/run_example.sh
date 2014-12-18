export PYTHONPATH=../../distrib/python_osx/:..

if [ $1 -lt 10 ]; then
    FILENAME=`ls demo_0$1_* 2>/dev/null`
else
    FILENAME=`ls demo_$1_* 2>/dev/null`    
fi


if [ -e "$FILENAME" ]; then
    echo running demo script $FILENAME
    python $FILENAME
else
    echo requested script does not exist
fi