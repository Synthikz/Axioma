section .data
    global sample_program_data
    global sample_program_size

sample_program_data:
    incbin "build/sample.pask"
sample_program_size:
    dd __sample_program_end - sample_program_data

__sample_program_end:
