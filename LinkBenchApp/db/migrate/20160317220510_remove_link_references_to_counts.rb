class RemoveLinkReferencesToCounts < ActiveRecord::Migration
  def change
	remove_reference :counts, :link
  end
end
