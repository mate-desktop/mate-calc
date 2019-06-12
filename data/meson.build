msgfmt = find_program('msgfmt')

custom_target('desktop-file',
              output: 'mate-calc.desktop',
              input: files('mate-calc.desktop.in'),
              install: true,
              install_dir: join_paths([datadir,'applications']),
              command: [msgfmt, '--desktop', '-d', '../po', '--template', '@INPUT@', '-o', '@OUTPUT@'])

custom_target('appdata',
              output: 'mate-calc.appdata.xml',
              input: files('mate-calc.appdata.xml.in'),
              install: true,
              install_dir: join_paths([datadir,'appdata']),
              command: [msgfmt, '--xml', '-d', '../po', '--template', '@INPUT@', '-o', '@OUTPUT@'])

cs = gnome.compile_schemas()
install_data(	'org.mate.calc.gschema.xml',
                install_dir: schemadir)