class ProjectMAudioProcessor extends AudioWorkletProcessor {
	process(inputs, outputs, parameters) {
		const input = inputs[0];

		// Ensure we have audio data on the first channel
		if (input && input.length > 0) {
			const channelData = input[0]; // Float32Array containing PCM data

			// Send the raw float array to the main thread.
			// Note: AudioWorklets always process in fixed chunks of 128 frames.
			this.port.postMessage(channelData);
		}

		// Return true to keep the processor alive
		return true; 
	}
}

registerProcessor('projectm-audio-processor', ProjectMAudioProcessor);
