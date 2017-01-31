function [vocoded_block,s] = Vocode_block(block,p,s)
%Vocode_block runs a block through a CI vocoder
%
% Syntax:
%   [vocoded_block,s] = Vocode_block(block,p,s)
%
% Inputs:
%   block           - input block (size: blklen-by-1)
%   p               - parameters:
%                       p.fs            - sampling frequency
%                       p.n_channels
%                       p.filterbank
%                       p.lowpass
%                       p.carriers      - carriers cell
%   s               - states
%                       s.filterbank_state
%                       s.lowpass_state
%
% Outputs:
%   vocoded_block   - output block

% pass through filterbank (and update its states)
channels = zeros(p.n_channels,p.block_length);
for channel = 1:p.n_channels
    [channels(channel,:),s.filterbank_state(:,:,channel)] = apply_filter(block,p.filterbank(channel,:),s.filterbank_state(:,:,channel));
end

% full-wave rectification
rectified_channels = abs(channels);

% envelope extraction with lowpass (and update its states)
envelopes = zeros(size(rectified_channels));
for channel = 1:p.n_channels
    [envelopes(channel,:),s.lowpass_state(:,:,channel)] = apply_filter(rectified_channels(channel,:),p.lowpass,s.lowpass_state(:,:,channel));
end

% put on carrier and sum channels
carriers = zeros(size(envelopes));
if strcmp(p.carriers,'sines')
    for channel = 1:p.n_channels
        n_start = s.passed_blocks*p.block_length+1;
        n_end = (s.passed_blocks+1)*p.block_length;
        t = [n_start:n_end]/p.fs;
        f = p.center_frequencies(channel);
        carriers(channel,:) = sin(2*pi*f*t);
    end
else
    for channel = 1:p.n_channels
        n_start = s.passed_blocks*p.block_length+1;
        n_start = mod(n_start-1,length(p.carriers{channel}))+1;
        n_end = (s.passed_blocks+1)*p.block_length;
        n_end = mod(n_end-1,length(p.carriers{channel}))+1;
        if n_start<n_end
            carriers(channel,:) = p.carriers{channel}(n_start:n_end);
        else
            carriers(channel,:) = [p.carriers{channel}(n_start:end);p.carriers{channel}(1:n_end)];
        end
    end
end
vocoded_block = sum(envelopes.*carriers,1);

% increment block number (to avoid onsets in carriers)
s.passed_blocks = s.passed_blocks + 1;

end

function [filtered_block,filter_states] = apply_filter(block,filter,filter_states)
filter_order = (size(filter_states,1)-1)/2;
block_length = size(filter_states,2);

filtered_block = zeros(1,block_length);

delayline_input = 2:filter_order+1;
delayline_output = filter_order+3:2*filter_order+1;
for sample = 1:block_length
    % update states for this iteration, respectively:
    %       * new input block (for this iteration)
    %       * old input block (row 2:filter_order+1) (for this iteration)
    %       * old output block (row filter_order+3:2*filter_order+1) (for this iteration)
    filter_states(1,sample) = block(sample);
    prev_sample = mod((sample-2),block_length)+1;
    filter_states(delayline_input,sample) = filter_states(delayline_input-1,prev_sample);
    filter_states(delayline_output,sample) = filter_states(delayline_output-1,prev_sample);
    
%     for delay = 1:filter_order
%         filter_states(1+delay,sample) = filter_states(1+delay-1,prev_sample);
%     end
    
%     for delay = 2:filter_order
%         filter_states(filter_order+1+delay,sample) = filter_states(filter_order+1+delay-1,prev_sample);
%     end
    
    % pass through filterbank
    filtered_block(sample) = filter*filter_states(:,sample);
    
    % update states for next iteration with new output block
    next_sample = mod(sample,block_length)+1;
    filter_states(filter_order+2,next_sample) = filtered_block(sample);
end

end

