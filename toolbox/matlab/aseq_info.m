function [fields,identifiers,formats,fieldsizes,version]=aseq_info

fields={...
    'electrodes'
    'return_electrodes'
    'current_levels'
    'phase_widths'
    'phase_gaps'
    'periods'
    'channels'
    'magnitudes' };

identifiers={...
    'AELE'
    'RELE'
    'CURL'
    'PHWI'
    'PHGA'
    'PERI'
    'CHAN'
    'MAGN' };

formats={
    'schar'
    'schar'
    'uchar'
    'float32'
    'float32'
    'float32'
    'schar'
    'float32' };

fieldsizes=[1 1 1 4 4 4 1 4];     % number of bytes per frame

version=1.0;