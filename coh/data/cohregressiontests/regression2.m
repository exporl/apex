sequence = struct;
sequence.triggers = zeros(4, 1);
sequence.electrodes = zeros(4, 1);
sequence.references = zeros(4, 1);
sequence.periods = zeros(4, 1);
sequence.amplitudes = zeros(4, 1);
sequence.current_levels = zeros(4, 1);
sequence.phase_widths = zeros(4, 1);
sequence.phase_gaps = zeros(4, 1);

sequence.triggers(4) = 1;
sequence.electrodes(1) = 10;
sequence.electrodes(2) = 11;
sequence.electrodes(3) = 12;
sequence.electrodes(4) = 13;
sequence.references(1:4) = -1;
sequence.periods(1) = 110;
sequence.periods(2) = 120;
sequence.periods(3) = 130;
sequence.periods(4) = 140;
sequence.current_levels(1) = 22;
sequence.current_levels(2) = 23;
sequence.current_levels(3) = 24;
sequence.current_levels(4) = 25;
sequence.phase_widths(1:4) = 25;
sequence.phase_gaps(1:4) = 8;

sequence.metas = struct;
sequence.metas.bool = [ true, ];
sequence.metas.double = [ 10.83, ];
sequence.metas.int = [ 5, 10, ];
sequence.metas.string = { 'Gänsefüßchen', };
