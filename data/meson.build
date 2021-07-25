# Desktop file
i18n.merge_file(
  input: 'mate-calc.desktop.in',
  output: 'mate-calc.desktop',
  type: 'desktop',
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications'),
)

# Appdata
i18n.merge_file(
  input: 'mate-calc.appdata.xml.in',
  output: 'mate-calc.appdata.xml',
  type: 'xml',
  po_dir: po_dir,
  install: true,
  install_dir: join_paths(get_option('datadir'), 'metainfo')
)

# Gschema
 install_data('org.mate.calc.gschema.xml',
   install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas'),
 )

# Manpage
install_man(
  'mate-calc-cmd.1',
  'mate-calc.1'
)
