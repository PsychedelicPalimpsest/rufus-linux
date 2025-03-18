#  Rufus: The Reliable USB Formatting Utility
#  Copyright © 2025 PsychedelicPalimpsest
#
#  Convert (to the best of my ability) the assets of rufus.rc to 
#  GResources.xml so that resources can be used in cross-platform GCC. 
#  These resources are NEVER USED IN WINDOWS! In addition also does 
#  convert (to the best of my ability) the windows UI to a cross-platfrom 
#  GTK look alike.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
import os
import subprocess
from os.path import *
DEBUG=True


DIR = dirname(abspath(__file__))
RUFUS_RC = join(DIR, "rufus.rc")
RESOURCE_H = join(DIR, "resource.h")
GTK_ASSET_DIR = join(dirname(DIR), "res", "gtk_ui")

RESOURCE_OUTPUT = join(DIR, "GResources.xml")
GTK_UI_FILE = join(GTK_ASSET_DIR, "window.xml")

assert exists(RUFUS_RC), "rufus.rc MUST exist to be converted to xml!"



# ===========================
#        UI conversion    
# ===========================

# Get all the resouce ids what vscode autogen'd
def extractDefines():
    result = subprocess.run(["gcc", "-E", "-dD", RESOURCE_H], capture_output=True, text=True)
    defines = {}
    
    for line in result.stdout.splitlines():
        if line.startswith("#define"):
            # This is a dumb way to do it, but
            # is suprisingly efficent!
            _, key, *value = line.split(" ")
            defines[key] = "".join(value)

    return defines

RESOURCES = extractDefines()


def convertId(strId):
    if strId in RESOURCES:
        return "ID_" + RESOURCES[strId]
    else:
        print(f"WARNING: {strId} not in resources.h!")
        return strId


def parseUiLine(data):
    data = data.lstrip()
    # The name if the item ends after a space
    name_len = min(data.find(" "), data.find("\n"))
    name = data[:name_len]

    if data[name_len] == '\n':
        return (name, [], data[name_len:])
    data = data[name_len:].lstrip()

    arguments = [""]
    recent_comma = False
    inQuote = False
    i = 0
    for i, c in enumerate(data):
        if c == "\n" and not recent_comma:
            break
        if c == '"':
            inQuote = not inQuote
        recent_comma = c == "," and not inQuote
        if not recent_comma:
            arguments[-1] += c
        else:
            arguments.append("")
    return (name, list(map(lambda x: x.strip(), arguments)), data[i:])


def heightCorrect(name, args):
    args[4] = 1 * GRID_Y_PX_RATIO
    return (name, args)


# Small tweeks that need done to look right
OVERIDES = [
    (
        lambda name, args: name == "COMBOBOX",
        heightCorrect, None
    )
    

]
import math
# Glade refuses to open too big of a grid
GRID_X_PX_RATIO = 8
GRID_Y_PX_RATIO = 4
def gridAjust(x, y, w, h):
    return (
        math.ceil(int(x) / GRID_X_PX_RATIO),
        math.ceil(int(y) / GRID_Y_PX_RATIO),
        math.ceil(int(w) / GRID_X_PX_RATIO),
        math.ceil(int(h) / GRID_Y_PX_RATIO / 80)
        )

def text(ID, label, x, y, w, h, visable=True):
    x, y, w, h = gridAjust(x, y, w, h)
    ID = convertId(ID)
    return f"""
            <child>
                <object class="GtkLabel" id="{ID}">
                    <property name="label">{label}</property>
                    <property name="halign">start</property>
                    <property name="visible">{visable}</property>
                    <layout>
                        <property name="column">{x}</property>
                        <property name="column-span">{w}</property>
                        <property name="row">{y}</property>
                        <property name="row-span">{h}</property>
                    </layout>
                </object>
            </child>"""
def combobox(ID, x, y, w, h, visable=True):
    x, y, w, h = gridAjust(x, y, w, h)
    ID = convertId(ID)
    return f"""
            <child>
              <object class="GtkComboBoxText" id="{ID}">
                <property name="visible">{visable}</property>
                <property name="can-focus">False</property>
               <items>
                  <item id="0" translatable="yes">{ID}</item>
                  <item id="1" translatable="yes">bar</item>
                </items>
                <layout>
                    <property name="column">{x}</property>
                    <property name="column-span">{w}</property>
                    <property name="row">{y}</property>
                    <property name="row-span">{h}</property>
                </layout>
              </object>
            </child>
        """
def button(ID, label, x, y, w, h, visable=True):
    x, y, w, h = gridAjust(x, y, w, h)
    ID = convertId(ID)
    return f"""
            <child>
              <object class="GtkButton" id="{ID}">
                <property name="label" translatable="yes">{label}</property>
                <property name="visible">{visable}</property>
                <property name="can-focus">True</property>
                <property name="receives-default">True</property>
                <layout>
                    <property name="column">{x}</property>
                    <property name="column-span">{w}</property>
                    <property name="row">{y}</property>
                    <property name="row-span">{h}</property>
                </layout>
              </object>
            </child>
        """
def checkBox(ID, label, x, y, w, h, visable=True):
    x, y, w, h = gridAjust(x, y, w, h)
    ID = convertId(ID)
    return f"""
            <child>
              <object class="GtkCheckButton" id="{ID}">
                <property name="label" translatable="yes">{label}</property>
                <property name="visible">{visable}</property>
                <property name="can-focus">True</property>
                <property name="receives-default">True</property>
                <layout>
                    <property name="column">{x}</property>
                    <property name="column-span">{w}</property>
                    <property name="row">{y}</property>
                    <property name="row-span">{h}</property>
                </layout>
              </object>
            </child>
        """
def edittext(ID, x, y, w, h, style, visable=True):
    x, y, w, h = gridAjust(x, y, w, h)
    ID = convertId(ID)
    return f"""
        <child>
          <object class="GtkTextView" id="{ID}">
            <property name="visible">{visable}</property>
            <property name="can-focus">True</property>
            <!--- <property name="input-purpose">number</property> -->
            <layout>
                <property name="column">{x}</property>
                <property name="column-span">{w}</property>
                <property name="row">{y}</property>
                <property name="row-span">{h}</property>
            </layout>
          </object>
        </child>
        """
# If an element is in the list, return that after applying the predicate,
# otherwise return default.
def listGetMapOrDefault(lis, index, predicate, default = False):
    if len(lis) > index:
        return predicate(lis[index])
    return default



# Takes an index of aproximatly where the dialog is,
# and returns a parsed version of the data
def parseDialog(indicator_index, data):

    # Find where the dialog declairation starts
    true_start = None
    for offset in range(0, indicator_index):
        if data[indicator_index - offset:].startswith("IDD_DIALOG "):
            true_start = indicator_index - offset
            break
    assert true_start is not None, "Parse error"

    head = data[true_start:]

    info = {"elements": []}
    lname = " "
    while lname.upper() != "BEGIN":
        (lname, args, head) = parseUiLine(head)
        if len(args):
            info[lname] = args

    while True:
        if lname.upper() == "END":
            break
        (lname, args, head) = parseUiLine(head)
        info["elements"].append((lname, args))

    return info

def orArgParse(args):
    return list(map(lambda x: x.strip(), args.split("|")))
def generateXMLFromDialogElement(name, arguments):
    for test, act, _ in OVERIDES:
        if act and test(name, arguments):
            (name, arguments) = act(name, arguments)
    if "LTEXT" == name:
        # https://learn.microsoft.com/en-us/windows/win32/menurc/ltext-control
        return text(
            # Id           text                x                   y             w              h 
            arguments[1], arguments[0][1:-1], arguments[2], arguments[3], arguments[4], arguments[5], 
            # Check the style arg if the element is not visable, assume to be visable
            listGetMapOrDefault(arguments, 6, lambda x: "NOT WS_VISIBLE" not in x, default=True)
        )
    elif "COMBOBOX" == name:
        # https://learn.microsoft.com/en-us/windows/win32/menurc/combobox-control
        return combobox(
            # ID            x                  y         w           h
            arguments[0], arguments[1], arguments[2], arguments[3], arguments[4],
            listGetMapOrDefault(arguments, 5, lambda x: "NOT WS_VISIBLE" not in x, default=True)
        )
    elif "PUSHBUTTON" == name or "DEFPUSHBUTTON" == name:
        # https://learn.microsoft.com/en-us/windows/win32/menurc/pushbutton-control
        return button(
            # ID              text              x             y            w                 h
            arguments[1], arguments[0][1:-1], arguments[2], arguments[3], arguments[4], arguments[5],
            listGetMapOrDefault(arguments, 6, lambda x: "NOT WS_VISIBLE" not in x, default=True)
        )
    elif "EDITTEXT" == name:
        # https://learn.microsoft.com/en-us/windows/win32/menurc/edittext-control
        return edittext(
            # ID            x                y          w              h            style
            arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5],
            listGetMapOrDefault(arguments, 5, lambda x: "NOT WS_VISIBLE" not in x, default=True)
        )
    # https://learn.microsoft.com/en-us/windows/win32/menurc/control-control
    elif "CONTROL" == name:
        style = orArgParse(arguments[3])
        if '"button"' == arguments[2].lower() and (
                # Only buttons without anything special
                len(style) == 0 or (len(style) == 1 and style[0] == "WS_TABSTOP")
            ):
            return button(
                # ID                 text          x             y               w            h
                arguments[1], arguments[0][1:-1], arguments[4], arguments[5], arguments[6], arguments[7],
                "NOT WS_VISIBLE" not in style
                )
        elif '"button"' == arguments[2].lower() and "BS_AUTOCHECKBOX" in style:
            # print(style, arguments)
            return checkBox(
                # ID                 text          x             y               w            h
                arguments[1], arguments[0][1:-1], arguments[4], arguments[5], arguments[6], arguments[7],
                "NOT WS_VISIBLE" not in style
                )
        else:
            print(arguments, arguments[2].lower(), style)
            return "\n<!-- UNSUPPORTED CONTROL STYLE OF '" + arguments[2] + "'' -->"

    else:
        return "\n<!-- UNSUPPORTED ELEMENT <" + name + "> -->"
def generateXMLFromDialog(dialog_data):

    xml = """
<object class="GtkWindow" id="window">
    <property name="can-focus">False</property>
    <child>
        <object class="GtkGrid">
            <property name="visible">True</property>
            <property name="can-focus">False</property>

            <property name="column-homogeneous">False</property>
"""
    for name, args in dialog_data["elements"]:
        x = generateXMLFromDialogElement(name, args)
        for test, _, act in OVERIDES:
            if act and test(name, args):
                x = act(x, name, args)
        xml += x


    xml += "\n</object></child></object>"
    return xml

def generateGTKUI():
    data = open(RUFUS_RC, "r").read()
    f = open(GTK_UI_FILE, "w")
    dialog = parseDialog(data.find("CAPTION \"Rufus"), data)
    f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    f.write("<interface>")
    f.write('\t<requires lib="gtk+" version="4.0"/>')
    f.write(generateXMLFromDialog(dialog))
    f.write("</interface>")
    f.close()

    if DEBUG:
        import gi
        gi.require_version("Gtk", "4.0")
        from gi.repository import Gtk, Gio

        class MyApplication(Gtk.Application):
            def __init__(self):
                super().__init__(application_id="com.example.myapp")

            def do_activate(self):
                builder = Gtk.Builder.new_from_file(GTK_UI_FILE)
                window = builder.get_object("window")
                window.set_application(self)
                window.show()

        app = MyApplication()
        app.run(None)




# ===========================
#     Asset XML generator    
# ===========================

def isAsset(line):
    return " RCDATA " in line
def processAsset(line):
    # The name of the asset is terminated by many spaces
    firstSpace = line.find(" ")
    rName = line[:firstSpace]
    line = line[firstSpace:].lstrip() # 

    
    # Now line is at the RCDATA keyword
    nextSpace = line.find(" ")
    assert "RCDATA" == line[:nextSpace], "Unknown resource item"
    
    # Now we have the location of the asset, this little trick
    # allows us to evaluate any tricky string escapes.
    rLocInStr = line[nextSpace:].strip().encode("utf-8").decode('unicode_escape')

    assert rLocInStr.startswith('"') and rLocInStr.endswith('"'), "Invalid resource"

    return (rName, rLocInStr[1:-1])
PREFIX='../res/'
def generateResources():
    data = open(RUFUS_RC, "r").read()
    # We only care about the resources themselves, not the rest
    ti3_lines = data.split("// Generated from the TEXTINCLUDE 3 resource.")[-1].split("\n")

    ti3_data = map(processAsset, filter(isAsset, ti3_lines))
    
    f = open(RESOURCE_OUTPUT, "w")
    f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
    f.write('<!-- GResources XML file autogenerated by generate_unix_assets.py - DO NOT EDIT! -->\n')
    f.write('<gresources>\n')
    f.write(f'  <gresource prefix="/res/">\n')
    for alias, path in ti3_data:
        # Strip away annoying prefix
        assert path.startswith(PREFIX), "Unexpected resource locations"
        path = path[len(PREFIX):]
        
        f.write(f"    <file alias=\"{alias}\">{path}</file>\n")
    f.write('  </gresource>\n')
    f.write('</gresources>')
    f.close()

# TODO: Add to make files something for unix resources
# via: glib-compile-resources --target=unix_resources.c --generate-source unix_resources.xml --sourcedir=../res

if __name__ == "__main__":
    generateGTKUI()
    generateResources()