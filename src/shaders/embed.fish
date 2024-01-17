#!/bin/fish

set content (cat $argv[1])
set content (string split '\n' $content)

echo "const char *$argv[2] = "
for line in $content
    echo "    \"$line\n\""
end
echo ";"

