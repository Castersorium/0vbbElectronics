#!/bin/bash

# Generate the tree
tree=$(tree)

# Define the start and end markers
start_marker="<!-- TREE START -->"
end_marker="<!-- TREE END -->"

# Create a temporary file
temp_file=$(mktemp)

# Check if the start marker exists in the README.md file
if grep -q "$start_marker" README.md; then
    # If the start marker exists, replace the old tree with the new one
    awk -v var="$tree" '/<!-- TREE START -->/{p=1;print;print "```";print var;print "```"} /<!-- TREE END -->/{p=0} !p' README.md > $temp_file
else
    # If the start marker does not exist, append the new tree with markers
    cp README.md $temp_file
    echo -e "$start_marker\n\`\`\`\n$tree\n\`\`\`\n$end_marker" >> $temp_file
fi

# Replace the README.md file with the temporary file
mv $temp_file README.md


#echo "\`\`\`" >> README.md
#tree          >> README.md
#echo "\`\`\`" >> README.md

git add .

if [ "$1" == "-m" ] && [ ! -z "$2" ]
then
    # If -m option and a commit message are provided, perform the git commit
    git commit -m "$2"
    git log --graph --pretty=format:'%Cred%h%Creset - %C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit --date=relative --color=always | head -n 5

fi

if [ "$1" == "-h" ] 
then
    echo "You can try to git commit using the -m option"
    exit 1
fi
