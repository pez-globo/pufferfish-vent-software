"""Test the functionality of protocols.application.states classes."""

from ventserver.protocols.application import lists
from ventserver.protocols.protobuf import mcu_pb as pb


def test_sync_new_elements() -> None:
    """Test adding new elements to a list."""
    example_sequence = [
        lists.UpdateEvent(new_element=pb.LogEvent(id=i))
        for i in range(20)
    ]

    synchronizer = lists.SendSynchronizer(
        segment_type=pb.NextLogEvents,
        max_len=10, max_segment_len=5
    )
    assert synchronizer.output() is None
    for update_event in example_sequence:
        synchronizer.input(update_event)

    assert synchronizer.output() is None

    # The first 10 events should've been discarded
    for next_expected in range(10):
        synchronizer.input(lists.UpdateEvent(next_expected=next_expected))
        output = synchronizer.output()
        assert isinstance(output, pb.NextLogEvents)
        assert output.next_expected == next_expected
        assert output.total == 10
        assert output.remaining == 10
        for (i, event) in enumerate(output.elements):
            assert event.id == 10 + i

    # Segments should be returned as requested
    for next_expected in range(10, 20):
        print(next_expected)
        synchronizer.input(lists.UpdateEvent(next_expected=next_expected))
        output = synchronizer.output()
        assert isinstance(output, pb.NextLogEvents)
        assert output.next_expected == next_expected
        assert output.total == 10
        assert output.remaining == 10 - (next_expected - 10)
        for (i, event) in enumerate(output.elements):
            assert event.id == next_expected + i
        if next_expected <= 15:
            assert len(output.elements) == 5
        else:
            assert len(output.elements) == 5 - (next_expected - 15)

    # New elements should be in the segment resulting from a repeated request
    assert synchronizer.output() is None
    synchronizer.input(lists.UpdateEvent(
        new_element=pb.LogEvent(id=20), next_expected=19
    ))
    output = synchronizer.output()
    assert isinstance(output, pb.NextLogEvents)
    assert output.next_expected == 19
    assert output.total == 10
    assert output.remaining == 2
    for (i, event) in enumerate(output.elements):
        assert event.id == 19 + i
    assert len(output.elements) == 2
