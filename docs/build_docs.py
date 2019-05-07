#This script will build the documentation for the sde, which includes:
#
# - Generating Doxgyen documentation for the C++ APIs
#   - This is actually done by Sphinx
# - Parse the test examples into documentation

import os
import subprocess

def separate_file(filename):
    comments = []
    code = []
    with open(filename, 'r') as input_file:
        started_with = ""
        in_comment = False
        for line in input_file:
            if "/*" in line:
                if started_with == "":
                    started_with = "comment"
                in_comment = True
                comments.append(line)
                code.append("")
            elif "*/" in line:
                in_comment = False
                comments[-1]+=line
            elif in_comment:
                comments[-1]+=line
            else:
                if started_with == "":
                    started_with = "code"
                    code.append("")
                code[-1] += line
    return started_with, comments, code

def print_code(output_file, code):
    if code.strip() == "":
        return
    output_file.write(".. code:: c++\n\n")
    for line in code.split('\n'):
        output_file.write("    " + line +'\n')

def print_comment(output_file, comment):
    for cmt in comment.split('\n'):
        cmt = cmt.strip()
        cmt = cmt.replace("/* ", '')
        cmt = cmt.replace("*/", '')
        cmt = cmt.replace("* ", '')
        if cmt == '*':
            cmt = ""
        output_file.write(cmt +'\n')

def write_tutorials(examples_dir, tutorial_dir):
    tutorial_list = []

    for filename in os.listdir(examples_dir):
        if filename.endswith(".hpp"):

            #Parse the example
            infile = os.path.join(examples_dir, filename)
            started_with, comments, code = separate_file(infile)

            #Determine where to write the tutorial to
            clean_name = filename.replace("Test",'').split(".hpp")[0]
            tutorial_list.append(clean_name)
            outfile = clean_name + ".rst"
            outfile = os.path.join(tutorial_dir, outfile)

            with open(outfile, 'w') as output_file:
                #Write the page's title
                title = clean_name + " Tutorial"
                output_file.write(title + '\n')
                output_file.write('='*len(title) + "\n\n")

                #We assume the file went:
                #   - comment, code, comment, ... or
                #   - code, comment, code, ...
                #This means for n lines of code, and allowing a closing remark,
                #we have:
                #   - n or n + 1 comments if we started with a comment
                #   - n or n-1 comments if we started with a line of code
                #In both cases the number of code/comments can differ by at
                #most 1

                if abs(len(code) - len(comments)) > 1:
                    raise Exception("# of comments/snippets don't make sense")

                #To unify the algorithms insert an empty comment before the
                #first code block if we started with code, now we have n or
                #n + 1 comments
                if started_with == "code":
                    comments = [""] + comments

                for i in range(len(comments)):
                   print_comment(output_file, comments[i])
                   output_file.write('\n')
                   if len(code) > i: #Possible to have one less line of code
                      print_code(output_file, code[i])
                      output_file.write('\n')

    with open(os.path.join(tutorial_dir, "index.rst"), 'w') as f:
        f.write("List of Tutorials\n")
        f.write("=================\n\n")
        f.write(".. toctree::\n")
        f.write("   :maxdepth: 2\n")
        f.write("   :caption: Contents:\n\n")
        for t in tutorial_list:
            f.write("   " + t + "\n")



def main():
     docs_dir = os.path.dirname(os.path.realpath(__file__))
     root_dir = os.path.dirname(docs_dir)
     examples_dir = os.path.join(root_dir, "tests", "examples")
     tutorial_dir = os.path.join(docs_dir, "source", "tutorials")

     if not os.path.exists(tutorial_dir):
         os.mkdir(tutorial_dir)

     write_tutorials(examples_dir, tutorial_dir)
     subprocess.call(["make", "html"])

if __name__ == "__main__" :
    main()
