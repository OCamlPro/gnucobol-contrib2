import os


class EnvLoader:
    env_dict = {'bp_file': '/full/path/to/xanim_breakpoint.lst',  # full path to breakpoint list
                'src_folder': '/path/to/modules',                 # directory with COBOL modules
                'ui_file': 'bpedit.ui',                           # path to Qt5 UI file
                'libcob_path': '/path/to/libcob'                  # path to directory containing libcob
               }

    def __init__(self):
        self.__load_config()

    def __load_config(self):
        if os.path.isfile('paths.ini') == False:
            return;
        with open('paths.ini') as f:
            for line in f.readlines():
                if line[0] == '#' or line[0] == ';':
                    continue
                splitted_line = line.split('=')
                key = splitted_line[0].strip()
                if key in self.env_dict:
                    self.env_dict[key] = splitted_line[1].strip()

    def get_breakpoints_file(self):
        return self.env_dict['bp_file']

    def get_src_folder(self):
        return self.env_dict['src_folder']

    def get_ui_file(self):
        return self.env_dict['ui_file']

    def init_path(self):
        path = os.environ.get('PATH', '-1')
        os.environ['PATH'] = '{};{};{}'.format(path, self.get_src_folder(), self.env_dict['libcob_path'])
