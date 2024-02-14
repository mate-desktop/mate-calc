project('mate-calc','c',
  version: '1.28.0',
  meson_version: '>=0.50.0',
  license: 'GPLv3+',
)

i18n = import('i18n')
gnome = import('gnome')

po_dir = join_paths(meson.source_root(), 'po')
datadir = join_paths([ get_option('prefix'),get_option('datadir') ])
pkgdatadir = join_paths([ get_option('prefix'),get_option('datadir'),'mate-calc' ])
bindir = join_paths([ get_option('prefix'), 'bin' ])
schemadir = join_paths([ datadir, 'glib-2.0', 'schemas' ])
locale_dir = join_paths(get_option('prefix'), get_option('localedir'))

# Dependencies
glib_min_version = '2.40.0'

gio = dependency('gio-2.0', version: '>= ' + glib_min_version)
glib = dependency('glib-2.0', version: '>= ' + glib_min_version)
gobject = dependency('gobject-2.0', version: '>= ' + glib_min_version)
libxml = dependency('libxml-2.0')
gtk = dependency('gtk+-3.0', version : '>=3.22')

# Libraries
cc = meson.get_compiler('c')
mpc = declare_dependency(
  dependencies: [
    cc.find_library('mpc'),
  ]
)
mpfr = declare_dependency(
  dependencies: [
    cc.find_library('mpfr'),
  ]
)
conf = configuration_data()

top_inc = include_directories('.')

conf.set('G_LOG_USE_STRUCTURED', true)
conf.set_quoted('PACKAGE_NAME', 'mate-calc')
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('GETTEXT_PACKAGE', 'mate-calc')
conf.set_quoted('PACKAGE_URL', 'https://mate-desktop.org')
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
subdir('data')
subdir('po')
subdir('help')

meson.add_install_script('meson-postinstall.sh')
