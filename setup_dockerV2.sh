FUSTR_dir="$(dirname "$BASH_SOURCE")"
echo $FUSTR_dir
target=${1%/}

cp -r $target $FUSTR_dir
echo cp -r  $target $FUSTR_dir/$(basename $target)
echo $FUSTR_dir

echo $work_dir
ls $work_dir
docker build -t  fustr --build-arg package=$(basename $target)  $FUSTR_dir --no-cache
#this file has to be adjusted

##


#1. **`FUSTR_dir="$(dirname "$BASH_SOURCE")"`**: 
#This line gets the directory name of the script using `dirname` and stores it in the variable `FUSTR_dir`.

#2. **`echo $FUSTR_dir`**: This line prints the value of the `FUSTR_dir` variable to the console.

#3. **`target=${1%/}`**: This line removes trailing slashes from the first command-line argument and 
#stores the result in the `target` variable.

#4. **`cp -r $target $FUSTR_dir`**: This line recursively copies the 
#contents of the `target` directory to the `FUSTR_dir`.

#5. **`echo cp -r  $target $FUSTR_dir/$(basename $target)`**: 
#This line echoes the command that would be used to copy the contents of `target` to `FUSTR_dir` along with the basename of `target`.

#6. **`echo $FUSTR_dir`**: This line prints the value of `FUSTR_dir` to the console.

#7. **`echo $work_dir`**: This line prints the value of `work_dir` to the console. 
#However, `work_dir` is not defined in the provided snippet, so its value is not clear.

#8. **`ls $work_dir`**: This line lists the contents of the directory specified by `work_dir`.

#9. **`docker build -t  fustr --build-arg package=$(basename $target)  $FUSTR_dir --no-cache`**: 
#This line builds a Docker image named `fustr`. It uses the `--build-arg` option to pass the base name of the `target` directory as a build argument.

 #  - `-t fustr`: Specifies the name of the image as "fustr."
  # - `--build-arg package=$(basename $target)`: Passes the base name of `target` as a build argument named "package."
  # - `$FUSTR_dir`: Specifies the build context directory.
  # - `--no-cache`: Requests that Docker does not use the cache during the build.

#The script seems to involve copying files, printing information, and building a Docker image. 
#The specifics may depend on the context and the values of variables like `work_dir`, which is not defined in the provided snippet. 
#Make sure to adjust the script as needed for your specific use case.