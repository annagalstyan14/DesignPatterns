#pragma once

#include <string>

/**
 * @brief Command Pattern Interface
 * 
 * Encapsulates an action as an object, enabling:
 * - Undo/redo functionality
 * - Command history tracking
 * - Decoupling of invoker from action execution
 * 
 * Design Pattern: Command
 * - ConcreteCommands: ApplyEffectCommand, EffectStateCommand
 * - Invoker: CommandHistory
 * - Receiver: AudioClip, EffectsPanel
 */
class ICommand {
public:
    virtual ~ICommand() = default;
    
    /**
     * @brief Execute the command
     * 
     * First execution should save state for undo.
     * Subsequent executions (after undo) should restore to post-execution state.
     */
    virtual void execute() = 0;
    
    /**
     * @brief Undo the command
     * 
     * Restore state to before execute() was called.
     */
    virtual void undo() = 0;
    
    /**
     * @brief Redo the command
     * 
     * Default implementation calls execute().
     * Override if redo needs different behavior than execute.
     */
    virtual void redo() { execute(); }
    
    /**
     * @brief Get human-readable description of the command
     * 
     * @return Description for display in UI (e.g., "Apply Reverb Effect")
     */
    [[nodiscard]] virtual std::string getDescription() const = 0;
};
