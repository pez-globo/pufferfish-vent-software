import { PBMessageType } from '../types';

// Send state update scheduling

export interface ScheduleEntry {
  time: number;
  pbMessageType: PBMessageType;
}

export type Schedule = Array<ScheduleEntry>;

// Note: this function mutates the input array
export const advanceSchedule = (schedule: Schedule): ScheduleEntry => {
  const entry = schedule.shift();
  if (entry === undefined) {
    throw new Error('State Synchronization: invalid schedule');
  }
  schedule.push(entry);
  return entry;
};
