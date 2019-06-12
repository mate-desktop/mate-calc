project('mate-calc','c', version: '1.23.0', meson_version: '>=0.36.0')

i18n = import('i18n')
gnome = import('gnome')

conf = configuration_data()

top_inc = include_directories('.')

deps = [ 
    dependency('gtk+-3.0', version : '>=3.22'),
    dependency('libxml-2.0'),
    dependency('gmodule-2.0'),
    dependency('glib-2.0')
]

datadir = join_paths([ get_option('prefix'),
                       get_option('datadir') ])
pkgdatadir = join_paths([ get_option('prefix'),
                          get_option('datadir'),
                          'mate-calc' ])
bindir = join_paths([ get_option('prefix'), 'bin' ])
schemadir = join_paths([ datadir, 'glib-2.0', 'schemas' ])

conf.set('G_LOG_USE_STRUCTURED', true)
conf.set_quoted('G_LOG_DOMAIN', 'org.mate.calc')
conf.set_quoted('PACKAGE_NAME', 'mate-calc')
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('GETTEXT_PACKAGE', 'mate-calc')
conf.set_quoted('DATADIR', datadir)
conf.set_quoted('UI_DIR', join_paths([pkgdatadir,'uidir']))
conf.set_quoted('PKGDATADIR', pkgdatadir)
conf.set_quoted('LOCALE_DIR', join_paths([ get_option('prefix'),
                                          get_option('datadir'),
                                          'locale' ]))

conf.set_quoted('pkgdatadir', pkgdatadir)
conf.set_quoted('bindir', bindir)

configure_file(output : 'config.h', configuration : conf)

subdir('src')
subdir('po')
subdir('data')
subdir('help')

meson.add_install_script('meson-postinstall.sh')
