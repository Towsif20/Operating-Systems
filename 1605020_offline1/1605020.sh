#!/bin/bash

wdir="dir"

if [ $# -eq 0 ]
then    
    echo "no arguments"
fi

if [ $# -eq 2 ]
then
    wdir=$1
    
    if [  ! -f "$2" ]
    then
        echo "not valid file, enter a valid name"
        exit
        
    fi


    file=$2

fi

if [ $# -eq 1 ]
then
    

    if [  ! -f "$1" ]
    then
        echo "not valid file"
        exit
    fi



    file=$1

fi

echo $wdir
echo $file

i=1

while read line;
do
    if [ $i -eq 1 ]
    then
        pos=$line

    elif [ $i -eq 2 ]
    then   
        lines=$line

    else
        string=$line

    fi

    i=$((i+1))

done < $file

echo $pos
echo $lines
echo $string

i=$((lines+1))
echo $i

expr $lines + 1

out_dir="output"
out_csv="output.csv"

rm -r $out_dir

mkdir $out_dir

rm "$out_csv"
touch "$out_csv"

echo "File Path, Line Number, Line containing searched string" >> "$out_csv"


traverse()
{
    for f in "$1"/*
    do
        if [ -d "$f" ]
        then
            traverse "$f";

        elif [ -f "$f" ]
        then
            line_no=0
            echo "file $f"

            if [ $pos == "begin" ] 
            then

                head -n "$lines" "$f" > temp.txt

                

                x=$(grep -n -i -m1 "$string" temp.txt)

                if [ ! "$x" == "" ]
                then

                    echo $x

                    line_no=$( echo "$x" | cut -d ':' -f1 )

                    echo $line_no

                    #line_content=$(grep -i -m1 -h "$string" temp.txt)

                    #echo $line_content

                    grep -n -i "$string" "temp.txt" > matchTemp.txt

                    echo matchTemp.txt

                    while read line 
                    do
                        csv_line=$(echo "$line" | cut -d ':' -f1)
                        line_content=$(echo "$line" | cut -d ':' -f2)
                        echo "${f},${csv_line},${line_content}" >>"$out_csv"


                    done < matchTemp.txt

        

                    # if [ ! $line_no -eq 0 ]
                    # then
                    #     echo "${f},${line_no},${line_content}" >>"$out_csv"

                    # fi

                fi

            
            else
                
                
                total=$(cat "$f" | wc -l)
                echo "total : " $total

                t1=$lines

                if [ $total -lt $lines ]
                then   
                    lines=$total
                fi

                tail -n "$lines" "$f" > temp.txt

                x=$(grep -i -n "$string" temp.txt | tail -1)

                if [ ! "$x" == "" ]
                then

                    grep -n -i "$string" "temp.txt" > matchTemp.txt

                    while read line
                    do
                        temp=$( echo "$line" | cut -d ':' -f1 )

                        line_content=$( echo "$line" | cut -d ':' -f2 )

                        echo $s

                        csv_line=$((total - lines + temp))

                        echo "${f},${csv_line},${line_content}" >>"$out_csv"

                        
                    done < matchTemp.txt


                    echo "last line : " $x

                    temp2=$( echo "$x" | cut -d ':' -f1 )

                    

                    line_content=$( echo "$x" | cut -d ':' -f2 )

                    echo $s

                    line_no=$((total - lines + temp2))

                    echo $lines

                    echo $line_no

                    # if [ ! $line_no -eq 0 ]
                    # then
                    #     echo "${f},${line_no},${line_content}" >>"$out_csv"

                    # fi
                fi 

                lines=$t1



            fi

        fi

        

        ext="$(echo ${f} | cut -d '.' -f2)"

        if [ -n "$ext" ]
        then   
            if [ "$ext" == "$f" ]
            then
                ext=""

            else
                ext=".${ext}"
            fi
            
        fi

        filename=$(echo $f | cut -d '.' -f1 | sed -e 's/\//./g' -e 's/ //g')
        echo $filename

        nothing=$(echo $filename | cut -d '.' -f1)

        if [ "$nothing" == "" ]
        then
            filename=$(echo $filename | sed 's/.//')
        fi

        if [ ! $line_no -eq 0 ]
        then
            newfile="${out_dir}/${filename}${line_no}${ext}"

            touch $newfile
            cp -r "$f" "$newfile"
        fi




    done


}

traverse $wdir

total=$(cat "$out_csv" | wc -l)

expr $total - 1