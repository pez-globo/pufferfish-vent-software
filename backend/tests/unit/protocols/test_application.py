"""Test the functionality of protocols.application classes."""

import collections

from ventserver.protocols import application
from ventserver.protocols.protobuf import mcu_pb as pb


def test_sync_inputs() -> None:
    """Test state updating from input sequences."""
    example_sequence = [
        (
            pb.Ping,
            application.StateUpdateEvent(time=i, pb_message=pb.Ping(id=i))
        ) for i in range(8)
    ]

    synchronizer = application.StateSynchronizer(
        output_schedule=collections.deque([
            application.ScheduleEntry(time=1.0, type=pb.Ping)
        ])
    )
    assert synchronizer.output() is None
    for (message_type, update_event) in example_sequence:
        synchronizer.input(update_event)
        assert synchronizer.current_time == update_event.time
        assert synchronizer.all_states[message_type] == update_event.pb_message


def update_clock(
        synchronizer: application.StateSynchronizer, time: float
) -> None:
    """Update the clock of the synchronizer."""
    synchronizer.input(application.StateUpdateEvent(time=time))


def test_sync_outputs_singletype() -> None:
    """Test single-type output scheduling from input sequences."""
    synchronizer = application.StateSynchronizer(
        output_schedule=collections.deque([
            application.ScheduleEntry(time=1.0, type=pb.Ping),
            application.ScheduleEntry(time=2.0, type=pb.Ping),
            application.ScheduleEntry(time=3.0, type=pb.Ping),
        ])
    )
    # schedule has not yet started
    assert synchronizer.output_schedule[0].time == 1.0
    assert synchronizer.output_deadline is None
    assert synchronizer.output() is None
    assert synchronizer.output_schedule[0].time == 1.0
    assert synchronizer.output_deadline is None
    # schedule entry 1
    synchronizer.input(application.StateUpdateEvent(
        time=0, pb_message=pb.Ping(id=1)
    ))
    assert synchronizer.current_time == 0
    assert synchronizer.output_schedule[0].time == 1.0
    assert synchronizer.output_deadline == 1.0
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=1)
    update_clock(synchronizer, 0.9)
    assert synchronizer.output() is None
    update_clock(synchronizer, 1.0)
    assert synchronizer.output() == pb.Ping(id=1)
    assert synchronizer.output() is None
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=1)
    # schedule entry 2
    assert synchronizer.output_schedule[0].time == 2.0
    assert synchronizer.output_deadline == 3.0
    update_clock(synchronizer, 1.5)
    assert synchronizer.output() is None
    synchronizer.input(application.StateUpdateEvent(pb_message=pb.Ping(id=2)))
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=2)
    update_clock(synchronizer, 3.0)
    assert synchronizer.output() == pb.Ping(id=2)
    assert synchronizer.output() is None
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=2)
    # schedule entry 3
    assert synchronizer.output_schedule[0].time == 3.0
    assert synchronizer.output_deadline == 6.0
    update_clock(synchronizer, 4.0)
    assert synchronizer.output() is None
    synchronizer.input(application.StateUpdateEvent(pb_message=pb.Ping(id=3)))
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=3)
    update_clock(synchronizer, 5.0)
    assert synchronizer.output() is None
    synchronizer.input(application.StateUpdateEvent(pb_message=pb.Ping(id=4)))
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=4)
    update_clock(synchronizer, 6.0)
    assert synchronizer.output() == pb.Ping(id=4)
    assert synchronizer.output() is None
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=4)
    # schedule entry 1
    assert synchronizer.output_schedule[0].time == 1.0
    assert synchronizer.output_deadline == 7.0
    synchronizer.input(application.StateUpdateEvent(pb_message=pb.Ping(id=5)))
    assert synchronizer.output() is None
    update_clock(synchronizer, 7.0)
    assert synchronizer.output() == pb.Ping(id=5)
    assert synchronizer.output() is None
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=5)


def test_sync_outputs_multitype() -> None:
    """Test multi-type output scheduling from input sequences."""
    synchronizer = application.StateSynchronizer(
        output_schedule=collections.deque([
            application.ScheduleEntry(time=0.1, type=pb.Ping),
            application.ScheduleEntry(time=0.9, type=pb.Announcement),
        ])
    )
    # schedule has not yet started
    assert synchronizer.output_schedule[0].time == 0.1
    assert synchronizer.output_deadline is None
    assert synchronizer.output() is None
    # schedule entry 1
    synchronizer.input(application.StateUpdateEvent(
        time=0, pb_message=pb.Ping(id=1)
    ))
    synchronizer.input(application.StateUpdateEvent(
        pb_message=pb.Announcement(announcement=b'1')
    ))
    assert synchronizer.current_time == 0
    assert synchronizer.output_schedule[0].time == 0.1
    assert synchronizer.output_deadline == 0.1
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=1)
    assert synchronizer.all_states[pb.Announcement] == pb.Announcement(
        announcement=b'1'
    )
    update_clock(synchronizer, 0.05)
    assert synchronizer.output() is None
    update_clock(synchronizer, 0.1)
    assert synchronizer.output() == pb.Ping(id=1)
    # schedule entry 2
    assert synchronizer.output_schedule[0].time == 0.9
    assert synchronizer.output_deadline == 1.0
    update_clock(synchronizer, 0.5)
    assert synchronizer.output() is None
    synchronizer.input(application.StateUpdateEvent(pb_message=pb.Ping(id=2)))
    assert synchronizer.all_states[pb.Ping] == pb.Ping(id=2)
    update_clock(synchronizer, 1.0)
    assert synchronizer.output() == pb.Announcement(announcement=b'1')
    # schedule entry 1
    assert synchronizer.output_schedule[0].time == 0.1
    assert synchronizer.output_deadline == 1.1
    update_clock(synchronizer, 1.05)
    assert synchronizer.output() is None
    update_clock(synchronizer, 1.1)
    assert synchronizer.output() == pb.Ping(id=2)
    # schedule entry 2
    assert synchronizer.output_schedule[0].time == 0.9
    assert synchronizer.output_deadline == 2.0
    assert synchronizer.output() is None
    update_clock(synchronizer, 2.0)
    assert synchronizer.output() == pb.Announcement(announcement=b'1')
