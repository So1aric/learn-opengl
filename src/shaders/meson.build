embed = find_program('./embed.fish', native: true)

shaders = [
    'common.vert',
    'common.frag',
]

foreach name : shaders
    output = name.underscorify() + '_src.h'
    path = join_paths(meson.current_source_dir(), name)
    files += custom_target(
        output,
        command: [embed, path, name.underscorify() + '_src'],
        output: output,
        capture: true,
        install: true,
        install_dir: './',
        build_always_stale: true,
    )
endforeach

