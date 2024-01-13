embed = find_program('./embed.fish', native: true)

shaders = [
    'common.vert',
    'common.frag',
]

foreach name : shaders
    output = name.underscorify() + '_src.h'
    files += custom_target(
        output,
        command: [embed, name, name.underscorify() + '_src'],
        output: output,
        capture: true,
    )
endforeach

