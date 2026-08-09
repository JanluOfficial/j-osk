project('j-osk', 'cpp',
  version: '0.1',
  default_options: ['cpp_std=c++20']
)

# Dependencies
gtk4_dep = dependency('gtk4')
layer_shell_dep = dependency('gtk4-layer-shell-0')

# Peel code generation
peel_gen = find_program('peel-gen')

# gnome for GResource complication
gnome = import('gnome')

run_command('mkdir', 'build/peel')
peel_files = custom_target('peel-gen',
  output: 'peel',
  command: [
    peel_gen,
    '--out-dir', '@OUTDIR@', # Use @OUTDIR@ to ensure it writes into the build folder
    '--recursive',
    'Gtk', '4.0',
    'Gtk4LayerShell', '1.0'
  ]
)

# Find all C++ files in src/
#src_files = run_command('find', 'src', '-name', '*.cpp', check: true).stdout().strip().split('\n')
src_files = run_command('find', 'src', '-name', '*.cpp', 
  check: true).stdout().strip().split('\n')

# Compile the resources
resources = gnome.compile_resources(
  'josk-resources',
  'resources/resources.xml',
  source_dir: 'resources',
  c_name: 'josk'
)

executable('j-osk',
  src_files,
  peel_files,
  resources,
  include_directories: [
    include_directories('include'),
    include_directories('build/peel')
  ],
  dependencies: [gtk4_dep, layer_shell_dep],
  install: true
)
