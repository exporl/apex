sequence = struct;
sequence.triggers = zeros(4, 1);
sequence.electrodes = zeros(4, 1);
sequence.references = zeros(4, 1);
sequence.periods = zeros(4, 1);
sequence.amplitudes = zeros(4, 1);
sequence.current_levels = zeros(4, 1);
sequence.phase_widths = zeros(4, 1);
sequence.phase_gaps = zeros(4, 1);

sequence.triggers(1) = 1;
sequence.periods(1:4) = 51;
sequence.amplitudes(1) = -2048;
sequence.amplitudes(2) = -1;
sequence.amplitudes(4) = 1;
sequence.amplitudes(3) = 2047;
