src = []
test_mp_src = []
test_mp_eq_src = []

enums = []

enums += gnome.mkenums('types',
    sources: 'mp-serializer.h',
    c_template: 'mp-enums.c.template',
    h_template: 'mp-enums.h.template')

ui_files = files(
    'buttons-advanced.ui',
    'buttons-basic.ui',
    'buttons-financial.ui',
    'preferences.ui'
)

ui_resources = gnome.compile_resources(
    'ui_resources',
    'org.mate.calculator.gresource.xml',
    source_dir: 'src'
)


src += [
    'mate-calc.c',
    'currency-manager.c',
    'currency.c',
    'financial.c',
    'lexer.c',
    'math-buttons.c',
    'math-converter.c',
    'math-display.c',
    'math-equation.c',
    'math-preferences.c',
    'math-variable-popup.c',
    'math-variables.c',
    'math-window.c',
    'mp-binary.c',
    'mp-convert.c',
    'mp-equation.c',
    'mp-trigonometric.c',
    'mp-serializer.c',
    'mp.c',
    'parser.c',
    'parserfunc.c',
    'prelexer.c',
    'unit-category.c',
    'unit-manager.c',
    'unit.c',
    enums,
    ui_resources
]

test_mp_src += [
    'test-mp.c',
    'mp.c',
    'mp-binary.c',
    'mp-convert.c',
    'mp-serializer.c',
    'mp-serializer.h',
    'mp-trigonometric.c',
    enums
]

test_mp_eq_src += [
    'test-mp-equation.c',
    'currency.c',
    'currency.h',
    'currency-manager.c',
    'currency-manager.h',
    'mp.c',
    'mp-convert.c',
    'mp-binary.c',
    enums,
    'mp-equation.c',
    'mp-serializer.c',
    'mp-serializer.h',
    'mp-trigonometric.c',
    'unit.c',
    'unit.h',
    'unit-category.c',
    'unit-category.h',
    'unit-manager.c',
    'unit-manager.h',
    'prelexer.c',
    'prelexer.h',
    'lexer.c',
    'lexer.h',
    'parserfunc.c',
    'parser.c',
    'parser.h'
]

executable('mate-calc', src, install : true, include_directories: top_inc,
    dependencies: deps, link_args: '-lm')

executable('test-mp', test_mp_src, include_directories: top_inc, 
    dependencies: deps, link_args: '-lm')

executable('test-mp-equation', test_mp_eq_src, include_directories: top_inc,
    dependencies: deps, link_args: '-lm')

