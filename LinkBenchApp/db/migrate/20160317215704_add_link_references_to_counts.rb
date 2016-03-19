class AddLinkReferencesToCounts < ActiveRecord::Migration
  def change
	add_reference :counts, :link, index: true, source: :linktype
  end
end
