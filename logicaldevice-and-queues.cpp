// After selecting a physical device we need to set up a logical device to interact with it. 
// The logical device creation process is similar to the instance creation process. 
// We will also specify which queues to create now that we queried the families.

// Add a new class member to store the logical device handle in.

	VkDevice device;
