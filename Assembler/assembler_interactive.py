import assembler

if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        if sys.argv[1].lower() == "<ask>":
            file = input("Input file: ")
        else:
            file = sys.argv[1]

        assembler.main(file, "", interactive=True)
    else:
        print("Assembly file is unspecified")