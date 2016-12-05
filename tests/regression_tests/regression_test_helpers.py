import os.path, filecmp, difflib

def compare_graphs(name, output_type):
    filename = name + '.' + output_type
    filename_reference = 'reference/' + filename
    filename_output = 'output/' + filename
    if not os.path.isfile('reference/' + filename):
        print('Failure: ' + filename + ' - No reference file present.')
        return False

    with open(filename_reference) as reference_file:
        with open(filename_output) as output_file:
            reference = reference_file.readlines()
            output = output_file.readlines()
            diff_generator = difflib.context_diff(output, reference, filename_output, filename_reference)

            # if diff contains at least one line, the files are different
            diff = []
            for line in diff_generator:
                diff.append(line)

            if len(diff) == 0:
                print('Success: ' + filename)
                return True
            else:
                if not os.path.exists('difference'):
                    os.makedirs('difference')

                # Write diff to console
                for line in diff:
                    print(line)

                # Store diff in file
                with open('difference/' + filename, 'w') as diff_file:
                    diff_file.writelines(diff)

                print('Failure: ' + filename + ' - Generated file does not match reference file.')
                return False
