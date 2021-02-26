#  ===========================================================================
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================
from Tkinter import *
import os


class Application(Frame):

    def __init__(self, master):
        Frame.__init__(self, master)
        self.grid(sticky=N+W+E+S)
        self.create_widgets()
        self.columnconfigure(1, weight=1)

    def create_widgets(self):
        row = 1

        label = Label(self, text="hansei.py Path:")
        label.grid(row=row, column=0, sticky=W)

        self.hansei_path = Entry(self, width=100)
        self.hansei_path.grid(row=row, column=1, sticky=W+E)
        this_dir = os.path.dirname(os.path.abspath(__file__))
        self.hansei_path.insert(0, os.path.join(this_dir, 'hansei.py'))

        row += 1

        label = Label(self, text="ELF Path:")
        label.grid(row=row, column=0, sticky=W)

        self.elf_path = Entry(self, width=100)
        self.elf_path.grid(row=row, column=1, sticky=W+E)

        elf_dir = os.path.join(os.sep.join(this_dir.split(os.sep)[:-2]), 'build')
        self.elf_path.insert(0, os.path.join(elf_dir, 'AOP_AAAAANAAO.elf'))

        row += 1

        label = Label(self, text="Dump Path:")
        label.grid(row=row, column=0, sticky=W)

        self.dump_path = Entry(self, width=100)
        self.dump_path.grid(row=row, column=1, sticky=W+E)

        row += 1

        label = Label(self, text="Target:")
        label.grid(row=row, column=0, sticky=W)

        self.target = Entry(self)
        self.target.grid(row=row, column=1, sticky=W+E)

        row += 1

        label = Label(self, text="Output Path:")
        label.grid(row=row, column=0, sticky=W)

        self.out_path = Entry(self, width=100)
        self.out_path.grid(row=row, column=1, sticky=W+E)

        row += 1

        self.run_btn = Button(self, text="Run Hansei")
        self.run_btn.grid(row=row, column=0, columnspan=2)
        self.run_btn['command'] = self.run_hansei

    def run_hansei(self):
        cmd = 'python %s --elf %s -t %s --output %s dumpfile %s' % (
                self.hansei_path.get(),
                self.elf_path.get(),
                self.target.get(),
                self.out_path.get(),
                self.dump_path.get(),
                )
        print
        print 'Hansei Command: %s' % cmd
        ret = os.system(cmd)
        if ret == 0:
            os.system('explorer %s' % self.out_path.get())
        print 'Done'


if __name__ == '__main__':
    root = Tk()
    root.title("Hansei GUI")
    root.geometry("800x150")
    root.columnconfigure(0, weight=1)

    app = Application(root)

    root.mainloop()


