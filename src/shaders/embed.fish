#!/bin/fish

set content (od -A n -t x1 -v $argv[1])
set content (string split ' ' $content)

echo -n "const unsigned char *$argv[2] = { "
for ch in $content
    echo -n "$ch, "
end
echo "};"

