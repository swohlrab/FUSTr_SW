FUSTrDev_dir=$HOME/FUSTrDev
echo $FUSTrDev_dir
target=${1%/}

cp -r $target $FUSTrDev_dir
echo cp -r $target $FUSTrDev_dir/$(basename $target)
echo $FUSTrDev_dir

echo $work_dir
ls $work_dir
docker build -t  fustr --build-arg package=$(basename $target)  $FUSTrDev_dir --no-cache
